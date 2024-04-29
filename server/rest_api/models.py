from django.db import models
from django.contrib.auth import get_user_model

DATA_LOGGER_TYPE = [
    ("prototype_v1", "prototype_v1"),
    ("commerical", "commercial"),
    ("commercial_gps", "commercial_gps"),
]


class Location(models.Model):
    name = models.CharField(max_length=100, blank=True, null=True)
    latitude = models.FloatField()
    longitude = models.FloatField()

    def __str__(self):
        return f"{self.name} ({self.latitude}, {self.longitude})"


class DataLogger(models.Model):
    user = models.ForeignKey(to=get_user_model(), on_delete=models.SET_NULL, null=True)
    default_location = models.ForeignKey(
        to=Location, blank=True, on_delete=models.SET_NULL, null=True
    )
    type = models.CharField(choices=DATA_LOGGER_TYPE, max_length=100)


class Reading(models.Model):
    data_logger = models.ForeignKey(to=DataLogger, on_delete=models.SET_NULL, null=True)
    location = models.ForeignKey(
        to=Location, blank=True, on_delete=models.SET_NULL, null=True
    )
    time_stamp = models.DateTimeField()
    temperature = models.FloatField()
    humidity = models.PositiveIntegerField()
    raw_reading = models.PositiveIntegerField()
    co2_ppm = models.PositiveIntegerField()


4
