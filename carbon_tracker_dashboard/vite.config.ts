// @ts-
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import dotenv from 'dotenv'

dotenv.config()
// https://vitejs.dev/config/
export default defineConfig({
  build: {},
    // @ts-ignore
  base: process.env["environment"] == "development" ? "/" : "/static/",
  // @ts-ignore
  root: process.env["environment"] == "development" ? "./":  "../server/dashboard/templates/",
  plugins: [react()]
})
