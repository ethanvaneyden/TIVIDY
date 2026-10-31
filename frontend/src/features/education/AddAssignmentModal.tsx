import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface AddAssignmentModalProps {
    show: boolean;
    busy: boolean;
    onHide: () => void;
    onSubmit: (name: string) => Promise<boolean>;
}

function AddAssignmentModal({ show, busy, onHide, onSubmit }: AddAssignmentModalProps) {
    const [name, setName] = useState('');
    const trimmed = name.trim();

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (!trimmed) return;
        if (await onSubmit(trimmed)) {
            setName('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add assignment or test</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="assignment-name">Assignment or test name</Form.Label>
                    <Form.Control id="assignment-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required autoFocus />
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" className="workflow-primary-button" disabled={busy || !trimmed}>
                        {busy ? 'Adding...' : 'Add assignment / test'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddAssignmentModal;
