# Generated by Django 5.0.4 on 2024-04-29 17:05

import django.db.models.deletion
from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('rest_api', '0005_dataset'),
    ]

    operations = [
        migrations.AddField(
            model_name='reading',
            name='dataset',
            field=models.ForeignKey(null=True, on_delete=django.db.models.deletion.SET_NULL, to='rest_api.dataset'),
        ),
        migrations.AlterField(
            model_name='datalogger',
            name='type',
            field=models.CharField(choices=[('ESP8266', 'ESP8266'), ('ESP32', 'ESP32')], max_length=100),
        ),
    ]
