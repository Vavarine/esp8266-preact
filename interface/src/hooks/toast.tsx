import { ToastContainer } from '@/components/Toast/ToastContainer'
import { createContext } from 'preact'
import type { ComponentChildren } from 'preact'
import { useCallback, useContext, useState } from 'preact/hooks'

export interface ToastMessage {
	id: string
	type?: 'success' | 'error' | 'info'
	title?: string
	description?: string
}

interface ToastContextData {
	addToast(message: Omit<ToastMessage, 'id'>): void
	removeToast(id: string): void
}

const ToastContext = createContext<ToastContextData>({} as ToastContextData)

const ToastProvider = ({ children }: { children: ComponentChildren }) => {
	const [messages, setMessages] = useState<ToastMessage[]>([])

	const addToast = useCallback(
		({ type, title, description }: Omit<ToastMessage, 'id'>) => {
			const id = window.crypto.randomUUID()

			if (!title && !description) {
				return
			}

			const toast = {
				id,
				type,
				title,
				description
			}
			setMessages((oldMessages) => [...oldMessages, toast])
		},
		[]
	)

	const removeToast = useCallback((id: string) => {
		setMessages((state) => state.filter((message) => message.id !== id))
	}, [])

	return (
		<ToastContext.Provider value={{ addToast, removeToast }}>
			{children}
			<ToastContainer messages={messages} />
		</ToastContext.Provider>
	)
}

function useToast() {
	const context = useContext<ToastContextData>(ToastContext)

	if (!context) {
		throw new Error('useToast must be used within a ToastProvider')
	}

	return context
}

export { ToastProvider, useToast }
