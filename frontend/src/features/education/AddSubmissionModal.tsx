import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';
import type { NewSubmission } from '../../api/workflow';

interface AddSubmissionModalProps {
    show: boolean;
    busy: boolean;
    assignmentName?: string;
    onHide: () => void;
    onSubmit: (submission: NewSubmission) => Promise<boolean>;
}

function AddSubmissionModal({ show, busy, assignmentName, onHide, onSubmit }: AddSubmissionModalProps) {
    const [studentName, setStudentName] = useState('');
    const trimmed = studentName.trim();

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (!trimmed) return;
        if (await onSubmit({ studentName: trimmed })) {
            setStudentName('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton>
                    <Modal.Title>Add submission{assignmentName ? ` to ${assignmentName}` : ''}</Modal.Title>
                </Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="student-name">Student name</Form.Label>
                    <Form.Control id="student-name" value={studentName} onChange={(event) => setStudentName(event.target.value)} maxLength={80} required autoFocus />
                </Modal.Body>
                <Modal.Footer>
                    <Button variant="outline-secondary" onClick={onHide} disabled={busy}>Cancel</Button>
                    <Button type="submit" className="workflow-primary-button" disabled={busy || !trimmed}>
                        {busy ? 'Adding...' : 'Add submission'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddSubmissionModal;
