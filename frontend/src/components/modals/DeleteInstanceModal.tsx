import type { SubmitEvent } from 'react';
import { useState } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface DeleteInstanceModalProps {
    show: boolean;
    instanceName?: string;
    onHide: () => void;
    onConfirm: () => Promise<void>;
}

function DeleteInstanceModal({ show, instanceName, onHide, onConfirm }: DeleteInstanceModalProps) {
    const [busy, setBusy] = useState(false);

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        setBusy(true);
        await onConfirm();
        setBusy(false);
        onHide();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Delete instance</Modal.Title></Modal.Header>
                <Modal.Body>
                    <p>Are you sure you want to delete “{instanceName ?? 'this instance'}”?</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>
                        Cancel
                    </Button>
                    <Button type="submit" variant="danger" disabled={busy}>
                        {busy ? 'Deleting…' : 'Delete instance'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default DeleteInstanceModal;