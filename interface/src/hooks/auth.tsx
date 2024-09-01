import { createContext } from 'preact'
import type { ComponentChildren } from 'preact'
import { useCallback, useContext, useEffect, useState } from 'preact/hooks'

interface User {
	username: string
	noAuth?: boolean
}

interface AuthState {
	token?: string
	user: User
}

interface SignInCredentials {
	username: string
	password: string
}

interface AuthContextData {
	user: User | undefined
	signIn(credentials: SignInCredentials): Promise<void>
	signOut(): Promise<void>
}

const AuthContext = createContext<AuthContextData>({} as AuthContextData)

export const AuthProvider = ({ children }: { children: ComponentChildren }) => {
	const [data, setData] = useState<AuthState | undefined>(undefined)

	useEffect(() => {
		fetch('/api/profile')
			.then((res) => {
				if (res.ok) {
					return res.json()
				}

				throw new Error('Failed to fetch')
			})
			.then((data) => setData({ user: data }))
			.catch(() => setData(undefined))
	}, [])

	const signIn = useCallback(
		async ({ username, password }: SignInCredentials) => {
			const response = await fetch('/api/session', {
				method: 'POST',
				headers: {
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({
					username,
					password
				})
			})

			if (!response.ok) {
				throw new Error('Failed to sign in')
			}

			const data = await response.json()

			const { token, user } = data

			setData({ token, user })
		},
		[]
	)

	const signOut = useCallback(async () => {
		await fetch('/api/logout')

		setData(undefined)
		window.location.reload()
	}, [])

	return (
		<AuthContext.Provider value={{ user: data?.user, signIn, signOut }}>
			{children}
		</AuthContext.Provider>
	)
}

export function useAuth(): AuthContextData {
	const context = useContext(AuthContext)

	if (!context) {
		throw new Error('useAuth must be used within an AuthProvider')
	}

	return context
}
