import { NavLink } from "react-router-dom";
interface NavigationProps {
  //   children: ReactNode;
}

export default function Navigation({ }: NavigationProps) {
  return (
    <nav className="flex flex-row xl:flex-col items-center px-8 md:px-16 xl:px-0">
      <NavLink
        to="/dashboard"
        className="flex flex-row xl:flex-col items-center bg-green-800 px-4 xl:px-0 xl:w-full xl:py-2 "
      >
        <img src="/logo.png" className=" h-16 w-[86px] xl:w-[171px] xl:h-32" width="auto" />
        <h1 className="hidden xl:block text-green-600 font-bold text-sm xl:text-xl tracking-widest text-center ">
          CARBON TRACKER
        </h1>
      </NavLink>
      <div className="text-white text-lg font-semibold tracking-widest py-2 w-fdivl xl:w-40 px-4 xl:px-0 xl:gap-2 text-center flex flex-row xl:flex-col gap-4">
        <NavLink to="/dashboard" className={({ isActive }) => isActive ? "border-b-2 border-b-white px-3 xl:pt-2" : "px-3 xl:pt-2"} >Dashboard</NavLink>
      </div>
    </nav>
  );
}
