from django.db.models import Avg, F
from django.db.models.functions import TruncHour
from rest_framework.response import Response
from device.models import DataLogger, Reading, Location
import datetime
from rest_framework import viewsets
import folium
import pandas as pd
import numpy as np
from scipy.interpolate import griddata
from folium.plugins import HeatMap
from django.template.loader import render_to_string
from django.http import HttpResponse
import humanize
from django.utils import timezone
from django.shortcuts import render


def frontend(request):
    return render(request, "index.html")


class KPIViewset(viewsets.ViewSet):
    def list(self, request):
        current = datetime.datetime.now()
        monthly_average = (
            Reading.objects.filter(
                time_stamp__month=current.month, time_stamp__year=current.year
            )
            .values("co2_ppm")
            .aggregate(avg=Avg("co2_ppm"))
        )["avg"]

        highest_ppm_location = (
            Reading.objects.values("dataset__location__name")
            .annotate(avg=Avg("co2_ppm"))
            .order_by("-avg")
            .first()["dataset__location__name"]
        )

        data = {
            "activeDevices": DataLogger.objects.count(),
            "monthlyAverage": monthly_average,
            "highestPPMLocation": highest_ppm_location,
        }

        return Response(data=data)


class YearlyLocationAverageViewset(viewsets.ViewSet):
    def list(self, request):
        current = datetime.datetime.now()
        monthly_average = (
            Reading.objects.filter(time_stamp__year=current.year)
            .values(location=F("dataset__location__name"))
            .annotate(co2_ppm=Avg("co2_ppm"))
            .order_by("dataset__location__name")
        )

        return Response(data=monthly_average)


class DailyByHourViewset(viewsets.ViewSet):
    def list(self, request):

        # current = datetime.datetime.now()

        hourly_avgs = (
            Reading.objects.filter(time_stamp__date=datetime.date(2024, 4, 28))
            .annotate(hour=TruncHour("time_stamp"))
            .values(hour=F("time_stamp__hour"))
            .annotate(avg=Avg("co2_ppm"))
            .order_by("hour")
        )

        return Response(data=hourly_avgs)


def heatmap_page(request):
    loc = Location.objects.aggregate(lat=Avg("latitude"), long=Avg("longitude"))
    m = folium.Map(
        location=[loc["lat"], loc["long"]], tiles="OpenStreetMap", zoom_start=13
    )

    heat_data = []
    for l in Location.objects.all():
        query = Reading.objects.filter(dataset__location=l.pk).aggregate(
            avg=Avg("co2_ppm")
        )
        heat_data.append([l.latitude, l.longitude, query["avg"]])

    data = pd.DataFrame(heat_data, columns=["latitude", "longitude", "value"])
    data["value"] = (data["value"] - data["value"].min()) / (
        data["value"].max() - data["value"].min()
    )

    # Extract latitude, longitude, and value columns
    points = data[["latitude", "longitude"]].values
    values = data["value"].values

    # Define grid
    grid_lat, grid_lon = np.mgrid[
        data["latitude"].min() : data["latitude"].max() : 100j,
        data["longitude"].min() : data["longitude"].max() : 100j,
    ]
    grid_values = griddata(points, values, (grid_lat, grid_lon), method="cubic")

    interpolated_heat_data = []
    for lat, lon, value in zip(grid_lat.ravel(), grid_lon.ravel(), grid_values.ravel()):
        if not np.isnan(value):
            interpolated_heat_data.append([lat, lon, value])

    # Add HeatMap layer to the map
    HeatMap(
        interpolated_heat_data,
        radius=5,
        blur=3,
        min_opacity=0.1,
        gradient={0.1: "blue", 0.5: "lime", 1: "orange"},
    ).add_to(m)

    return HttpResponse(m._repr_html_())


class LiveDataViewset(viewsets.ViewSet):
    def list(self, request):

        devices = DataLogger.objects.all()
        data = []
        for device in devices:
            last_reading = (
                Reading.objects.filter(dataset__data_logger=device.pk)
                .order_by("-time_stamp")
                .first()
            )
            data.append(
                {
                    "device": str(device.pk),
                    "temperature": last_reading.temperature,
                    "humidity": last_reading.humidity,
                    "co2_ppm": last_reading.co2_ppm,
                    "location": last_reading.dataset.location.name,
                    "at": humanize.naturaltime(
                        timezone.localtime(timezone.now()) - last_reading.time_stamp
                    ),
                }
            )

        return Response(data=data)
