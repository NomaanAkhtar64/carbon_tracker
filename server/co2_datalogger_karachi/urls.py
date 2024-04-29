from django.contrib import admin
from django.urls import path, include
from rest_framework import routers
from rest_api.views import ReadingViewset
from rest_framework.authtoken.views import obtain_auth_token

router = routers.DefaultRouter()
router.register(r"readings", ReadingViewset)


urlpatterns = [
    path("admin/", admin.site.urls),
    path("api/", include(router.urls)),
    path("login/", obtain_auth_token, name="login"),
]
