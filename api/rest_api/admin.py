from django.contrib import admin
from .models import Reading
from .actions.csv_export import export_as_csv


class ReadingAdmin(admin.ModelAdmin):
    list_display = [
        "id",
        "time_stamp",
        "raw_reading",
        "co2_ppm",
        "temperature",
        "humidity",
    ]
    actions = [export_as_csv()]


admin.site.register(Reading, ReadingAdmin)
