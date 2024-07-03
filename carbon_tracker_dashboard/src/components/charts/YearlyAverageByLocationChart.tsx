import { useQuery } from "@tanstack/react-query";
import { Bar } from "react-chartjs-2";

type YearlyAverageByLocation = {
  location: string;
  co2_ppm: string;
}[];

function useYearlyAverageByLocation() {
  const { isPending, error, data } = useQuery<YearlyAverageByLocation>({
    queryKey: ["ybl"],
    queryFn: () =>
      fetch("http://localhost:8000/dashboard/api/yearly-by-location/", {
        headers: { "Content-Type": "text/json" },
      }).then((res) => res.json()),
    refetchInterval: 30000,
  });

  return { isPending, error, data };
}



export default function YearlyAverageByLocationBarChart() {
  const { isPending, data, error } = useYearlyAverageByLocation();

  if (isPending || isPending) return "Loading...";
  if (error || error) return "An error has occurred: ";
  if (!data || !data) return "no data recieved";
  const colors = [
    "#15803D", // Green 1
    "#4ADE80", // Green 4
    "#16A34A", // Green 2
    "#336633", // Green 5
    "#22C55E", // Green 3
  ];
  return (
    <div className="h-[350px] w-full flex flex-col border-green-950 border-b border-l border-r">
      <Bar
        className="my-auto"
        options={{
          indexAxis: "y" as const,
          elements: {
            bar: {
              borderWidth: 0,
            },

          },
          scales: {
            x: {
              beginAtZero: false,
              suggestedMin: 350,
              grid: {
                color: "rgba(0,0,0,0)"
              },
              title: {
                display: true,
                text: "CO2 Concentration (ppm)",
              },
            },
            y: {
              grid: {
                color: "rgba(0,0,0,0)"
              },
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
              position: "right" as const,
              display: false
            },
            title: {
              display: false,
            },
          },
        }}
        data={{
          labels: data.map((d) => d.location),
          datasets: [
            {
              label: "dataset",
              data: data.map((d) => parseFloat(d.co2_ppm)),
              borderColor: "#22C55E",
              backgroundColor: data.map((_, i) => colors[i]),
            },
          ],
        }}
      />
    </div>
  );
}
