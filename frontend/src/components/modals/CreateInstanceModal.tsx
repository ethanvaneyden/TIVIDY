import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface CreateInstanceModalProps {
    show: boolean;
    busy: boolean;
    onHide: () => void;
    onSubmit: (name: string) => Promise<boolean>;
}

function CreateInstanceModal({ show, busy, onHide, onSubmit }: CreateInstanceModalProps) {
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
                <Modal.Header closeButton><Modal.Title>Create instance</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="instance-name">Instance name</Form.Label>
                    <Form.Control id="instance-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={60} required autoFocus />
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" className="workflow-primary-button" disabled={busy || !trimmed}>
                        {busy ? 'Creating...' : 'Create instance'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default CreateInstanceModal;
