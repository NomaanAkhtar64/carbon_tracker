export interface ReadingSerializer {
    id?: number;
    time_stamp: string;
    temperature: number;
    humidity: number;
    raw_reading: number;
    co2_ppm: number;
    request_ip: string;
    data_logger?: number | string | null;
}

