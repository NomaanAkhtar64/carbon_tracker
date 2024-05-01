from django.contrib import admin
from django.urls import path, include
from rest_framework import routers
from device.views import ReadingViewset
from rest_framework.authtoken.views import obtain_auth_token
from django.conf.urls.i18n import i18n_patterns

router = routers.DefaultRouter()
router.register(r"readings", ReadingViewset)


urlpatterns = [
    path("admin/", admin.site.urls),
    path("i18n/", include("django.conf.urls.i18n")),
    path("api/", include(router.urls)),
    path("login/", obtain_auth_token, name="login"),
]
# urlpatterns += i18n_patterns(path("admin/", admin.site.urls))
