import { Alert, Button, Spinner } from 'react-bootstrap';

interface LoadingProps {
    label: string;
}

export function Loading({ label }: LoadingProps) {
    return (
        <div className="workflow-loading">
            <Spinner animation="border" role="status" size="sm" />
            <span>{label}</span>
        </div>
    );
}

interface LoadFailureProps {
    title: string;
    message?: string;
    onRetry: () => void;
}

export function LoadFailure({ title, message, onRetry }: LoadFailureProps) {
    return (
        <Alert variant="danger">
            <Alert.Heading>{title}</Alert.Heading>
            <p className="mb-3">{message ?? 'The server returned no data.'}</p>
            <Button variant="outline-danger" size="sm" onClick={onRetry}>Try again</Button>
        </Alert>
    );
}
