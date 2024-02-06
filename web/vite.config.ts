import path from "path"
import { defineConfig } from 'vite'
import { TanStackRouterVite } from '@tanstack/router-vite-plugin'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [
    // ...,
    TanStackRouterVite(),
  ],
  resolve: {
    alias: {
      "@": path.resolve(__dirname, "./src", "@"),
    },
  },
})
