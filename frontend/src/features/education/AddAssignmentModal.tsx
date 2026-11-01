import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

// interface AddAssignmentModalProps {
//     show: boolean;
//     onHide: () => void;
// }

interface AddAssignmentModalProps {
    show: boolean;
    onHide: () => void;
    onSubmit: (name: string) => Promise<boolean>;
}

// function AddAssignmentModal({ show, onHide }: AddAssignmentModalProps) {
//     const [name, setName] = useState('');

//     function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
//         event.preventDefault();
//     }

//     return (
//         <Modal show={show} onHide={onHide} centered>
//             <Form onSubmit={handleSubmit}>
//                 <Modal.Header closeButton><Modal.Title>Add assignment or test</Modal.Title></Modal.Header>
//                 <Modal.Body>
//                     <Form.Label htmlFor="assignment-name">Assignment or test name</Form.Label>
//                     <Form.Control id="assignment-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={80} required />
//                     <p className="action-placeholder-note">This form is not connected yet. Nothing will be added.</p>
//                 </Modal.Body>
//                 <Modal.Footer>
//                     <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
//                     <Button type="submit" className="workflow-primary-button">Add assignment / test</Button>
//                 </Modal.Footer>
//             </Form>
//         </Modal>
//     );
// }

function AddAssignmentModal({ show, onHide, onSubmit }: AddAssignmentModalProps) {
    const [name, setName] = useState('');
    const [busy, setBusy] = useState(false);

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        setBusy(true);
        const ok = await onSubmit(name.trim());
        setBusy(false);
        if (ok) {
            setName('');
            onHide();
        }
    }

    return (
        <Modal show={show} onHide={onHide} centered>
            <Form onSubmit={handleSubmit}>
                <Modal.Header closeButton>
                    <Modal.Title>Add assignment or test</Modal.Title>
                </Modal.Header>
                <Modal.Body>
                    <Form.Label htmlFor="assignment-name">Assignment or test name</Form.Label>
                    <Form.Control
                        id="assignment-name"
                        value={name}
                        onChange={(event) => setName(event.target.value)}
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
                        disabled={busy || !name.trim()}
                    >
                        {busy ? 'Adding…' : 'Add assignment / test'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default AddAssignmentModal;