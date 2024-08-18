import { ToastMessage, useToast } from '@/hooks/toast'
import clsx from 'clsx'
import { useEffect } from 'preact/hooks'

interface Props {
	message: ToastMessage
}

const TOAST_STYLE_MAP: Record<
	NonNullable<ToastMessage['type']>,
	string
> = /*tw*/ {
	error: 'alert-error',
	success: 'alert-success',
	info: 'alert-info'
}

export function Toast({ message }: Props) {
	const { removeToast } = useToast()

	useEffect(() => {
		const timer = setTimeout(() => {
			removeToast(message.id)
		}, 3000)

		return () => {
			clearTimeout(timer)
		}
	}, [removeToast, message.id])

	return (
		<div class={clsx('alert flex flex-col gap-2', TOAST_STYLE_MAP[message.type ?? 'info'])}>
			{message.title && <h4 class="text-lg font-semibold">{message.title}</h4>}
			{message.description && <p class="text-sm">{message.description}</p>}
		</div>
	)
}
