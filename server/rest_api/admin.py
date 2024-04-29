from django.contrib import admin
from .models import Reading, DataLogger, Location
from .actions.csv_export import export_as_csv

# class LocationAdminInline(admin.TabularInline):
#     model = Location


class ReadingAdmin(admin.ModelAdmin):
    list_display = [
        "id",
        "data_logger",
        "time_stamp",G
        "raw_reading",
        "co2_ppm",
        "temperature",
        "humidity",
    ]
    list_filter = ["data_logger"]
    actions = [export_as_csv()]


class DataLoggerAdmin(admin.ModelAdmin):
    list_display = ["id", "user", "default_location"]


class LocationAdmin(admin.ModelAdmin):
    list_display = ["id", "name", "latitude", "longitude"]


admin.site.register(Reading, ReadingAdmin)
admin.site.register(DataLogger, DataLoggerAdmin)
admin.site.register(Location, LocationAdmin)
