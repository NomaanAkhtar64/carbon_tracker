import { useQuery } from "@tanstack/react-query";
import { Line } from "react-chartjs-2";

type DailyAverageByDevice = {
  hour: number;
  avg: string;
}[];

function useDailyAverageByDevice() {
  const { isPending, error, data } = useQuery<DailyAverageByDevice>({
    queryKey: ["dbd"],
    queryFn: () =>
      fetch("http://localhost:8000/dashboard/api/daily-by-hour/", {
        headers: { "Content-Type": "text/json" },
      }).then((res) => res.json()),
    refetchInterval: 30000,
  });

  return { isPending, error, data };
}

export default function DailyAverageByDeviceLineChart() {
  const { isPending, data, error } = useDailyAverageByDevice();

  if (isPending || isPending) return "Loading...";
  if (error || error) return "An error has occurred: ";
  if (!data || !data) return "no data recieved";
  const colors = [
    "#15803D", // Green 1
    "#16A34A", // Green 2
    "#22C55E", // Green 3
    "#4ADE80", // Green 4
    "#86EFAC", // Green 5
  ];
  return (
    <div className="h-[350px] w-full flex flex-col border-green-950 border-b border-l border-r">
      <Line
        className="my-auto"
        options={{
          elements: {
            line: {
              borderWidth: 3,
            },
            point: {
              borderWidth: 0
            }
          },
          scales: {
            x: {
              title: {
                display: true,
                text: "CO2 Concentration (ppm)",
              },
            },
            y: {
              ticks: {
                font: {
                  size: 12,
                },
              },
            },
          },
          responsive: true,
          plugins: {
            legend: {
              position: "bottom" as const,
            },
            title: {
              display: false,
            },
          },
        }}
        data={{
          labels: data.map((d) => d.hour),
          datasets: [
            {
              label: "dataset",
              data: data.map((d) => parseFloat(d.avg)),
              borderColor: data.map((_, i) => colors[i]),
              backgroundColor: "#14532D",
            },
          ],
        }}
      />
    </div>
  );
}
