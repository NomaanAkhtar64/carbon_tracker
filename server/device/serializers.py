from rest_framework import serializers
from .models import Reading


class ReadingSerializer(serializers.ModelSerializer):
    class Meta:
        fields = "__all__"
        model = Reading
