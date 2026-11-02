import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

interface AddSubmissionModalProps {
    show: boolean;
    onHide: () => void;
    onSubmit: (studentName: string) => Promise<boolean>;
}

// function AddSubmissionModal({ show, onHide }: AddSubmissionModalProps) {
//     const [studentName, setStudentName] = useState('');

function AddSubmissionModal({ show, onHide, onSubmit }: AddSubmissionModalProps) {
    const [studentName, setStudentName] = useState('');
    const [busy, setBusy] = useState(false);

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        setBusy(true);
        const ok = await onSubmit(studentName.trim());
        setBusy(false);
        if (ok) {
            setStudentName('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton><Modal.Title>Add submission</Modal.Title></Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="student-name">Student name</Form.Label>
                    <Form.Control
                        id="student-name"
                        value={studentName}
                        onChange={(event) => setStudentName(event.target.value)}
                        maxLength={80}
                        required
                    />
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>
                        Cancel
                    </Button>
                    <Button
                        type="submit"
                        className="workflow-primary-button"
                        disabled={busy || !studentName.trim()}
                    >
                        {busy ? 'Adding…' : 'Add submission'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddSubmissionModal;