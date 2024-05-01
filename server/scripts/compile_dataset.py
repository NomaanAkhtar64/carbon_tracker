from device.models import Dataset, Reading


bottom_date = "2024-01-01"
top_date = "2024-02-01"

bottom2_date = "2024-02-01"


def run():
    # Dataset 1
    Reading.objects.filter(
        time_stamp__gte="2024-01-01", time_stamp__lte="2024-02-01"
    ).update(dataset=Dataset.objects.get(pk=1))

    Reading.objects.filter(time_stamp__gte="2024-02-01").update(
        dataset=Dataset.objects.get(pk=2)
    )
