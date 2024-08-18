import { defineConfig } from "vite";
import preact from "@preact/preset-vite";
import { espViteBuild } from "@ikoz/vite-plugin-preact-esp32";
import "dotenv/config";

const proxyTarget: string | undefined = process.env.VITE_PROXY;

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [espViteBuild(), preact()],
  resolve: {
    alias: {
      "@components": "/src/components",
      "@assets": "/src/assets",
      "@": "/src",
    },
  },
  server: proxyTarget ? {
    proxy: {
      "/api": {
        target: proxyTarget,
        changeOrigin: true,
      }
    }
  } : undefined
});
