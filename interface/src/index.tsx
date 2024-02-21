// import "preact/debug";
import { render } from "preact";
import { LocationProvider, Router, Route } from "preact-iso";

import { Header } from "./components/Header.jsx";
import { RemoteControl } from "./pages/RemoteControl/index.js";
import { NotFound } from "./pages/_404.jsx";
import { OtherPage } from "./pages/OtherPage/index.js";
import { Footer } from "./components/Footer.js";

import "./index.css";

export function App() {
  return (
    <LocationProvider>
      <Header />
      <main class="flex flex-col items-center mb-auto">
        <Router>
          <Route path="/" component={RemoteControl} />
          <Route path="/other-page" component={OtherPage} />
          <Route default component={NotFound} />
        </Router>
      </main>
      <Footer />
    </LocationProvider>
  );
}

render(<App />, document.getElementById("app"));
