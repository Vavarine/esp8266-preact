import { ToastMessage } from '@/hooks/toast'
import { Toast } from '.'

interface Props {
	messages: ToastMessage[]
}

export const ToastContainer = ({ messages }: Props) => {
	return (
		<div class="toast max-sm:toast-center sm:toast-end toast-bottom">
			{messages.map((message) => (
				<Toast key={message.id} message={message} />
			))}
		</div>
	)
}
