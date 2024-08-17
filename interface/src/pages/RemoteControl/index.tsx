import { useCallback, useEffect, useState } from 'preact/hooks'
import sunFilledIcon from '@assets/sun-filled.svg?raw'
import sunIcon from '@assets/sun.svg?raw'
import Icon from '@components/Icon'
import { api } from '@/utils/api'
import { useEnsureAuthCallback } from '@/hooks/useEnsureAuthCallback'
import { useToast } from '@/hooks/toast'
import { useAuth } from '@/hooks/auth'

interface LedData {
	state: '0' | '1'
}

export function RemoteControl() {
	const { addToast } = useToast()
	const { user } = useAuth()
	const [ledState, setLedState] = useState(false)

	const getLedState = async () => {
		const response = await fetch('/api/led')

		if (!response.ok) {
			return
		}

		const data = (await response.json()) as LedData

		setLedState(Boolean(data.state))
	}

	const handleButtonClick = useCallback(async () => {
		if (!user) {
			addToast({
				description: 'This action requires authentication',
				type: 'error'
			})
			return
		}

		const response = await fetch('/api/led/toggle')

		if (!response.ok) {
			addToast({
				description: 'You do not have permission to perform this action',
				type: 'error'
			})
			return
		}

		const data = (await response.json()) as LedData

		setLedState(Boolean(data.state))
	}, [])

	useEffect(() => {
		getLedState()
	}, [])

	return (
		<div class="container py-8 flex flex-col justify-center items-center">
			<button
				type="button"
				class="btn btn-lg btn-circle col-start-1 group"
				onClick={handleButtonClick}
				data-active={ledState}
			>
				<Icon class="group-data-[active='false']:hidden" icon={sunFilledIcon} />
				<Icon class="group-data-[active='true']:hidden" icon={sunIcon} />
			</button>
		</div>
	)
}
