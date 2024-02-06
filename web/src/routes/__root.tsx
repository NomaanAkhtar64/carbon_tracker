import { SideBar } from '@/components/component/side-bar'
import { createRootRoute } from '@tanstack/react-router'

export const Route = createRootRoute({
  component: () => (
    <SideBar/>
  ),
})