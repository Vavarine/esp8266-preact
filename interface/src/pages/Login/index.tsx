import { useAuth } from '@/hooks/auth'
import { useCallback, useEffect, useState } from 'preact/hooks'
import { useLocation } from 'preact-iso'
import type { JSX } from 'preact'
import clsx from 'clsx'
import { useToast } from '@/hooks/toast'

export function LoginPage() {
	const { user, signIn } = useAuth()
	const { addToast } = useToast()
	const location = useLocation()
	const [loading, setLoading] = useState(false)

	useEffect(() => {
		if (user) {
			location.route('/')
		}
	}, [user])

	const handleSubmit: JSX.GenericEventHandler<HTMLFormElement> = useCallback(
		async (e) => {
			e.preventDefault()

			const username = (
				e.currentTarget.elements.namedItem('username') as RadioNodeList
			)?.value
			const password = (
				e.currentTarget.elements.namedItem('password') as RadioNodeList
			)?.value

			if (!username || !password) {
				return
			}

			setLoading(true)
			try {
				await signIn({ username, password })
				addToast({
					description: 'You have successfully logged in',
					type: 'success'
				})
			} catch {
				addToast({
					description: 'Invalid username or password',
					type: 'error'
				})
				console.log('error')
			}

			setLoading(false)
		},
		[signIn]
	)

	return (
		<div class="container py-8 flex flex-col justify-center items-center">
			<form onSubmit={handleSubmit} class="flex flex-col gap-2">
				<input
					name="username"
					placeholder="Username"
					type="text"
					class="input input-bordered"
				/>
				<input
					name="password"
					placeholder="Password"
					type="password"
					class="input input-bordered"
				/>
				<button type="submit" class={clsx('btn', loading && 'btn-disabled')}>
					{loading ? (
						<span class="loading loading-spinner text-primary"></span>
					) : (
						<span>Log in</span>
					)}
				</button>
			</form>
		</div>
	)
}
