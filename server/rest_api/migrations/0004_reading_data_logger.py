# Generated by Django 5.0.1 on 2024-03-13 15:35

import django.db.models.deletion
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('rest_api', '0003_rename_locations_reading_location_location_name'),
    ]

    operations = [
        migrations.AddField(
            model_name='reading',
            name='data_logger',
            field=models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, to='rest_api.datalogger'),
        ),
    ]