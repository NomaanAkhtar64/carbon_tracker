from rest_framework import serializers
from .models import Reading
from django_typomatic import ts_interface, generate_ts


@ts_interface()
class ReadingSerializer(serializers.ModelSerializer):
    class Meta:
        fields = "__all__"
        model = Reading


generate_ts('../web/src/django-types.ts')