import { useQuery } from "@tanstack/react-query";

interface KeyPerformanceIndicators {
    activeDevices: number;
    monthlyAverage: number;


    highestPPMLocation: string;
}

function useKPIs() {
    const { isPending, error, data } = useQuery<KeyPerformanceIndicators>({
        queryKey: ["kpis"],
        queryFn: () =>
            fetch("http://localhost:8000/dashboard/api/kpi/", {
                headers: { "Content-Type": "text/json" },
            }).then((res) => res.json()),
        refetchInterval: 30000,
    });
    return { isPending, error, data };
}

export default function KPIsElement() {
    const { isPending, error, data } = useKPIs();
    if (isPending) return "Loading...";
    if (error) return "An error has occurred: ";
    if (!data) return "no data recieved";

    const today = new Date();

    return (
        <div className="xl:w-1/2 flex flex-col gap-2">
            <h2 className="text-green-100 font-semibold tracking-widest">KEY PERFORMANCE INDICATORS</h2>
            <div className="w-full h-full grid grid-cols-8 gap-2">
                <div className="flex flex-col items-start p-2 gap-2  col-span-3 bg-white rounded-md">
                    <span className="text-center w-full text-2xl xl:text-xl tracking-wider py-2">
                        {data.monthlyAverage.toFixed(1)} ppm
                    </span>
                    <span className="text-sm xl:text-xs text-green-800 w-full text-center">
                        Karachi Average Carbon (
                        {today.toLocaleString("default", { month: "long" })}{" "}
                        {today.getFullYear()})
                    </span>
                </div>
                <div className="flex flex-col items-start gap-2 p-2 col-span-2 bg-white rounded-md">
                    <span className="text-center w-full text-2xl xl:text-xl tracking-wider py-2">
                        {data.activeDevices}
                    </span>
                    <span className="text-sm w-full text-center text-green-800">
                        Active Devices
                    </span>
                </div>
                <div className="flex flex-col items-start p-2 gap-2 col-span-3 bg-white rounded-md">
                    <span className="text-center w-full text-2xl xl:text-xl tracking-wider py-2">
                        {data.highestPPMLocation}
                    </span>
                    <span className="text-sm xl:text-xs text-green-800 w-full text-center">
                        Highest Carbon Concentration
                    </span>
                </div>
            </div>
        </div>
    )
}