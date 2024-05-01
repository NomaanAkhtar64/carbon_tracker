from device.models import Reading
import datetime
from django.utils import timezone


def get_corrected_datetime(current_dt: datetime.datetime) -> datetime.datetime:
    output_dt = current_dt

    # Time was ahead 30min and is fixed by the next line
    output_dt = output_dt - datetime.timedelta(minutes=30)

    tz = timezone.get_current_timezone()
    output_dt = output_dt.combine(
        date=output_dt.date(),
        time=output_dt.time(),
        tzinfo=tz,
    )

    return output_dt


def run():
    reading_set = Reading.objects.all().in_bulk()
    count = Reading.objects.count()

    for i in reading_set.keys():
        corrected_dt = get_corrected_datetime(reading_set[i].time_stamp)
        reading_set[i].time_stamp = corrected_dt
        print(f"{i} of {count} - {100*i/count:.2f}% completed")

    Reading.objects.bulk_update(reading_set.values(), ["time_stamp"], batch_size=100)
