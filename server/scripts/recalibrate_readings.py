import math
from device.models import Reading, Dataset
from django.db.models.manager import BaseManager


def getVoltage1024(reading: int):
    return reading * (5.0 / 1024.0)


def get_ppm(reading: int, r400: int, r40000: int):

    v400 = getVoltage1024(r400)
    v40000 = getVoltage1024(r40000)
    v = getVoltage1024(reading)
    rV = v400 - v40000

    b = rV / math.log10(400 / 1000)
    a = (v - v400) / b + math.log10(400)

    # print(
    #     f"v400: {v400}, v40000: {v40000}, v: {v}, a: {a}, slope: {slope}, ppm: {10**a}"
    # )

    return round(pow(10, a))


def update_MG811_reading_set(reading_set: BaseManager[Reading], r400, r40000):
    update_set = reading_set.in_bulk()

    s = 0
    n = reading_set.count()
    for i in update_set.keys():
        co2ppm = get_ppm(update_set[i].raw_reading, r400, r40000)
        update_set[i].co2_ppm = co2ppm
        s += co2ppm

    if n == 0:
        avg = 0
    else:
        avg = s / n
    print(f"Average: {avg}")

    Reading.objects.bulk_update(update_set.values(), ["co2_ppm"], batch_size=100)


def run():
    update_MG811_reading_set(Reading.objects.filter(dataset=1), 835, 620)
    update_MG811_reading_set(Reading.objects.filter(dataset=2), 645, 494)
