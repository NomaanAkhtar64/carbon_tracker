// import { useState } from "react";
import Navigation from "../components/Navigation";
import { Outlet } from "react-router-dom";

function Root() {
  return (
    <div className=" w-full flex flex-col xl:flex-row min-h-screen h-full">
      <aside className="w-full xl:w-80  bg-green-800 xl:bg-neutral-700 flex-grow-0">
        <Navigation>Name</Navigation>
      </aside>
      <main className="w-full">
        <Outlet />
      </main>
    </div>
  );
}

export default Root;
