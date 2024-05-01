import csv
from django.contrib import admin
from .models import Reading, DataLogger, Location, Dataset
from .action import export_as_csv
from django.http import HttpResponse
from django.contrib.admin.utils import label_for_field
from django.utils.html import format_html
from django.urls import re_path, reverse
from django.utils import timezone

# import


class ReadingAdmin(admin.ModelAdmin):
    list_display = [
        "id",
        "dataset",
        "time_stamp",
        "raw_reading",
        "co2_ppm",
        "temperature",
        "humidity",
    ]
    list_filter = ["dataset"]
    actions = [export_as_csv()]


class DataLoggerAdmin(admin.ModelAdmin):
    list_display = ["id", "user"]


class LocationAdmin(admin.ModelAdmin):
    list_display = ["id", "name", "latitude", "longitude"]


class DataSetAdmin(admin.ModelAdmin):
    list_display = [
        "id",
        "location",
        "description",
        "data_logger",
        "average_ppm",
        "from_date",
        "to_date",
        "reading_count",
        "download_actions",
    ]
    fields = [
        "description",
        "data_logger",
        "location",
    ]
    readonly_fields = [
        "download_actions",
    ]

    def get_urls(self):
        urls = super(DataSetAdmin, self).get_urls()
        urls += [
            re_path(
                r"^download-csv/(?P<pk>\d+)$",
                self.download_csv,
                name="device_dataset_download-csv",
            ),
        ]

        return urls

    def download_actions(self, obj):
        return format_html(
            '<a href="{}">Download CSV</a>',
            reverse("admin:device_dataset_download-csv", args=[obj.pk]),
        )

    def download_csv(self, _, pk):
        response = HttpResponse(content_type="application/force-download")
        obj = Dataset.objects.get(pk=pk)
        response["Content-Disposition"] = f'attachment; filename="{str(obj)}.csv"'
        reading_set = Reading.objects.filter(dataset=pk)
        field_names = [
            "id",
            "time_stamp",
            "temperature",
            "humidity",
            "co2_ppm",
        ]

        writer = csv.writer(response)
        headers = []

        for field_name in field_names:
            label = label_for_field(field_name, Reading, ReadingAdmin)
            if str.islower(label):
                label = str.title(label)
            headers.append(label)
        writer.writerow(headers)

        for row in reading_set:
            values = []
            for field in field_names:
                value = getattr(row, field)
                if callable(value):
                    try:
                        value = value() or ""
                    except:
                        value = "Error retrieving value"
                if value is None:
                    value = ""

                if field == "time_stamp":
                    values.append(
                        str(value.astimezone(tz=timezone.get_current_timezone()))
                    )
                else:
                    values.append(str(value))
            writer.writerow(values)
        return response


admin.site.register(Dataset, DataSetAdmin)
admin.site.register(Reading, ReadingAdmin)
admin.site.register(DataLogger, DataLoggerAdmin)
admin.site.register(Location, LocationAdmin)
