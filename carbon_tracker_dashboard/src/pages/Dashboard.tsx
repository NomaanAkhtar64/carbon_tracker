import YearlyAverageByLocationBarChart from "../components/charts/YearlyAverageByLocationChart";
import DailyAverageByDeviceLineChart from "../components/charts/DailyAverageByDeviceLineChart";
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  BarElement,
  Title,
  Tooltip,
  Legend,
} from "chart.js";
import KPIsElement from "../components/KPI";
import { DataDisplay } from "../components/DataDisplay";

ChartJS.register(BarElement, CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
);

function Dashboard() {

  return (
    <div className="w-full h-full bg-white flex flex-col">
      <div className="px-8 py-4 border-b-2 border-green-900 bg-green-900">
        <h1 className="text-xl text-white font-semibold tracking-widest py-4 col-span-2">
          CARBON TRACKER - DASHBOARD
        </h1>
        <div className="flex flex-col xl:flex-row gap-2 xl:gap-8">
          <KPIsElement />
          <DataDisplay />
        </div>
      </div>
      <div className="px-16 py-8 bg-white h-full">
        <div className="flex flex-col xl:grid xl:grid-cols-6 gap-0">
          <div className="col-span-3 flex flex-col items-center p-2">
            <h3 className="text-green-900 font-semibold border border-green-950 w-full text-center py-2">
              Hourly Average Concentration
            </h3>
            <DailyAverageByDeviceLineChart />
          </div>
          <div className="col-span-3 flex flex-col items-center p-2">
            <h3 className="text-green-900 font-semibold border border-green-950 w-full text-center  py-2">
              Monthly Average Concentration by Area
            </h3>
            <YearlyAverageByLocationBarChart />
          </div>
          <div className="col-span-3 xl:col-span-6 flex flex-col items-center p-2">
            <div className="flex flex-row w-full justify-center relative items-center border border-green-950">
              <h3 className="text-green-900 font-semibold w-full text-center">
                Heat Map
              </h3>
              <button className=" text-sm text-green-900 border-green-950 border-l p-2" onClick={() => {
                const el = document.getElementById('heatmap') as HTMLIFrameElement
                if (el && el.contentWindow) {
                  el.contentWindow.location = "http://localhost:8000/dashboard/api/heatmap/"
                }
              }}>Reload</button>
            </div>
            <iframe id="heatmap" className="w-full h-full min-h-[450px] border-green-950 border-b border-l border-r" src="http://localhost:8000/dashboard/api/heatmap/" />
          </div>
        </div>
      </div>
    </div >
  );
}

export default Dashboard;
