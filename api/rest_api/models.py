from django.db import models
from django.contrib.auth import get_user_model



class DataLogger(models.Model):
    user = models.ForeignKey(to=get_user_model(), on_delete=models.SET_NULL, null=True)
    name = models.CharField(max_length=120)
    lastActive = models.DateTimeField(auto_now_add=True)
    is_active = models.BooleanField()

class Reading(models.Model):
    time_stamp = models.DateTimeField()
    temperature = models.FloatField()
    humidity = models.PositiveIntegerField()
    raw_reading = models.PositiveIntegerField()
    co2_ppm = models.PositiveIntegerField()
    data_logger = models.ForeignKey(to=DataLogger, on_delete=models.SET_NULL, null=True)
    request_ip = models.GenericIPAddressField()