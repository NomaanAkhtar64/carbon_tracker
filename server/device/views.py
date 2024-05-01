from rest_framework import viewsets
from rest_framework.permissions import IsAuthenticated
from .models import Reading
from .serializers import ReadingSerializer


class ReadingViewset(viewsets.ModelViewSet):
    permission_classes = [IsAuthenticated]
    queryset = Reading.objects.all()
    serializer_class = ReadingSerializer
