from rest_api.models import Reading, Dataset

# from multiprocessing.pool import ThreadPool
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
    # THREADS = 8
    # pool = ThreadPool(processes=THREADS)
    reading_set = Reading.objects.all()
    count = reading_set.count()
    # batch_size = round(count / 8)
    for i in range(count):
        corrected_dt = get_corrected_datetime(reading_set[i].time_stamp)
        reading_set[i].time_stamp = corrected_dt
        print(f"Old Time={reading_set[i].time_stamp}; New Time= {corrected_dt}")

    print(Reading.objects.bulk_update(reading_set, ["time_stamp"]))
