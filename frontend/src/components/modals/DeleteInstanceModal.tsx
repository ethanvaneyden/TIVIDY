import type { SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface DeleteInstanceModalProps {
    show: boolean;
    instanceName?: string;
    busy: boolean;
    onHide: () => void;
    onConfirm: () => Promise<boolean>;
}

function DeleteInstanceModal({ show, instanceName, busy, onHide, onConfirm }: DeleteInstanceModalProps) {
    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (await onConfirm()) onHide();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Delete instance</Modal.Title></Modal.Header>
                <Modal.Body>
                    <p className="mb-0">Are you sure you want to delete “{instanceName ?? 'this instance'}”?</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" variant="danger" disabled={busy}>
                        {busy ? 'Deleting...' : 'Delete instance'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default DeleteInstanceModal;
