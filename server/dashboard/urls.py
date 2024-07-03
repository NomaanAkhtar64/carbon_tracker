from django.urls import path, re_path, include
from rest_framework import routers
from .views import (
    KPIViewset,
    YearlyLocationAverageViewset,
    DailyByHourViewset,
    LiveDataViewset,
    heatmap_page,
    frontend,
)

router = routers.DefaultRouter()
router.register(r"kpi", KPIViewset, basename="kpi")
router.register(r"yearly-by-location", YearlyLocationAverageViewset, basename="ybl")
router.register(r"daily-by-hour", DailyByHourViewset, basename="dbh")
router.register(r"live-data", LiveDataViewset, basename="ld")

urlpatterns = [
    path("api/", include(router.urls)),
    path("api/heatmap/", heatmap_page),
    re_path(r"^.*$", frontend),
]
