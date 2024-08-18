// import "preact/debug";
import { render } from 'preact'
import { LocationProvider, Router, Route } from 'preact-iso'

import { Header } from './components/Header.jsx'
import { RemoteControl } from './pages/RemoteControl/index.js'
import { LoginPage } from './pages/Login/index.js'
import { NotFound } from './pages/_404.jsx'
import { OtherPage } from './pages/OtherPage/index.js'
import { Footer } from './components/Footer.js'
import { AuthProvider } from './hooks/auth.js'

import './index.css'
import { ToastProvider } from './hooks/toast.js'

export function App() {
	return (
		<LocationProvider>
			<ToastProvider>
				<AuthProvider>
					<Header />
					<main class="flex flex-col items-center mb-auto">
						<Router>
							<Route path="/login" component={LoginPage} />
							<Route path="/" component={RemoteControl} />
							<Route path="/other-page" component={OtherPage} />
							<Route default component={NotFound} />
						</Router>
					</main>
					<Footer />
				</AuthProvider>
			</ToastProvider>
		</LocationProvider>
	)
}

render(<App />, document.getElementById('app')!)
