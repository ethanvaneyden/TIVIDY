import type { SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface DeleteInstanceModalProps {
    show: boolean;
    instanceName?: string;
    onHide: () => void;
}

function DeleteInstanceModal({ show, instanceName, onHide }: DeleteInstanceModalProps) {
    function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Delete instance</Modal.Title></Modal.Header>
                <Modal.Body>
                    <p>Are you sure you want to delete “{instanceName ?? 'this instance'}”?</p>
                    <p className="action-placeholder-note mb-0">Deletion is not connected yet. Nothing will be deleted.</p>
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
                    <Button type="submit" variant="danger">Delete instance</Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default DeleteInstanceModal;
