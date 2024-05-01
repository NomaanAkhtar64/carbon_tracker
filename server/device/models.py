from django.db import models
from django.contrib.auth import get_user_model

DATA_LOGGER_TYPE = [
    ("ESP8266", "ESP8266"),
    ("ESP32", "ESP32"),
]


class Location(models.Model):
    name = models.CharField(max_length=100, blank=True, null=True)
    latitude = models.FloatField()
    longitude = models.FloatField()

    def __str__(self):
        return f"{self.name} ({self.latitude}, {self.longitude})"


class DataLogger(models.Model):
    user = models.ForeignKey(to=get_user_model(), on_delete=models.SET_NULL, null=True)
    type = models.CharField(choices=DATA_LOGGER_TYPE, max_length=100)

    def __str__(self) -> str:
        return f"{" ".join(map(str.capitalize, self.user.get_username().split("_")))} ({self.type})"


class Dataset(models.Model):
    location = models.ForeignKey(
        to=Location, blank=True, on_delete=models.SET_NULL, null=True
    )
    data_logger = models.ForeignKey(to=DataLogger, on_delete=models.SET_NULL, null=True)
    description = models.CharField(max_length=300)

    def __str__(self) -> str:
        return f"Dataset#{self.id} {self.location.name}"
    
    def reading_count(self):
        return Reading.objects.filter(dataset=self.pk).count()

    def average_ppm(self):
        acc_sum = 0
        n = 0
        for r in Reading.objects.filter(dataset=self.pk):
            acc_sum += r.co2_ppm
            n += 1 
        try:
            return acc_sum/n
        except ZeroDivisionError:
            return 0
    
    def from_date(self):
        return Reading.objects.filter(dataset=self.pk).earliest('time_stamp').time_stamp.date()
    
    def to_date(self):
        return Reading.objects.filter(dataset=self.pk).latest('time_stamp').time_stamp.date()
    



class Reading(models.Model):
    dataset = models.ForeignKey(to=Dataset, on_delete=models.SET_NULL, null=True)
    time_stamp = models.DateTimeField()
    temperature = models.FloatField()
    humidity = models.PositiveIntegerField()
    raw_reading = models.PositiveIntegerField()
    co2_ppm = models.PositiveIntegerField()


