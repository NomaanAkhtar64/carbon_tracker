import { useQuery } from "@tanstack/react-query";

type DataDisplay = {
    device: string;
    co2_ppm: number;
    temperature: number;
    humidity: number;
    location: string;
    at: string
}[]

function useDataDisplay() {
    const { isPending, error, data } = useQuery<DataDisplay>({
        queryKey: ["ld"],
        queryFn: () =>
            fetch("http://localhost:8000/dashboard/api/live-data/", {
                headers: { "Content-Type": "text/json" },
            }).then((res) => res.json()),
        refetchInterval: 100000,
    });

    return { isPending, error, data };
}
export function DataDisplay() {
    const { data, error, isPending } = useDataDisplay()

    if (isPending || isPending) return "Loading...";
    if (error || error) return "An error has occurred: ";
    if (!data || !data) return "no data recieved";

    return (
        <div className="w-full xl:w-1/2 flex flex-col gap-2">
            <h2 className="text-green-100 font-semibold tracking-widest">DATA DISPLAY</h2>
            <div className="w-full grid grid-cols-2 gap-2  h-full">
                {data.map((d, i) => (
                    <div className={" h-full grid grid-cols-7 grid-rows-2 items-center bg-white rounded-md " + (i == 0 ? "mr-2" : "")}>
                        <div className="relative row-span-2 h-full text-green-900">
                            <span className="text-nowrap font-bold text-r -rotate-90 text-sm absolute top-1/2 -translate-y-1/2">Device {d.device}</span>
                        </div>
                        <span className="text-center w-full text-2xl xl:text-lg tracking-wider p-2 col-span-4">
                            {d.co2_ppm && d.co2_ppm.toFixed(1)} ppm
                        </span>
                        <span className="text-center w-full xl:text-sm tracking-wider p-2 col-span-2 ">
                            T: {d.temperature} °C
                        </span>
                        <span className="text-sm xl:text-xs text-neutral-400 w-full text-center col-span-2">
                            {d.at}
                        </span>
                        <span className="text-sm xl:text-xs w-full text-center col-span-2 ">
                            {d.location}
                        </span>
                        <span className="text-center w-full  xl:text-sm tracking-wider p-2 col-span-2 ">
                            H: {d.humidity} %
                        </span>
                    </div>

                ))}

            </div>
        </div>
    )
}