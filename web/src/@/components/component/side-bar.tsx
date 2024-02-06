import { Link, Outlet } from '@tanstack/react-router'
import {BarChartIcon, FileTextIcon, HelpCircleIcon, HomeIcon, SettingsIcon} from 'lucide-react'
import { LogoIcon } from '@/icons/Logo'


export function SideBar() {
  return (
    <div className="flex h-screen flex-col md:flex-row bg-background">
      <div className="md:w-64 w-full bg-primary p-4 md:fixed md:h-screen">
        {/* <h2 className="text-lg font-semibold mb-4 text-primary-foreground pb-10">
          CO<sup>2</sup> LOGGING SYSTEM</h2> */}
        <LogoIcon className='md:w-full pb-10 w-64' />
        <nav className='gap-2 flex flex-col'>
          <Link className="flex items-center gap-2 py-2 px-4 rounded-lg  text-primary-foreground hover:bg-transparent-light" activeProps={{ className: "bg-transparent-light"}} to="/" >
            <HomeIcon className="w-4 h-4" />
            Dashboard
          </Link>
          <Link
            className="flex items-center gap-2 py-2 px-4  rounded-lg text-primary-foreground hover:bg-transparent-light" activeProps={{ className: "bg-transparent-light"}} to="/analytics"
          >
            <BarChartIcon className="w-4 h-4" />
            Analytics
          </Link>
          <Link
            className="flex items-center gap-2 py-2 px-4 rounded-lg text-primary-foreground hover:bg-transparent-light" activeProps={{ className: "bg-transparent-light"}} to="#"
          >
            <FileTextIcon className="w-4 h-4" />
            Reports
          </Link>
          <Link
            className="flex items-center gap-2 py-2 px-4 rounded-lg text-primary-foreground hover:bg-transparent-light" activeProps={{ className: "bg-transparent-light"}} to="#"
          >
            <SettingsIcon className="w-4 h-4" />
            Settings
          </Link>
          <Link
            className="flex items-center gap-2 py-2 px-4 rounded-lg text-primary-foreground hover:bg-transparent-light" activeProps={{ className: "bg-transparent-light"}} to="#"
          >
            <HelpCircleIcon className="w-4 h-4" />
            Support
          </Link>
        </nav>
      </div>
      <div className="md:ml-64 flex-1 p-6">
        <Outlet/>
      </div>
    </div>
  )
}
