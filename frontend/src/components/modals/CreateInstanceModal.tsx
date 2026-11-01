import { useState, type SubmitEvent } from 'react';
import { Button, Form, Modal } from 'react-bootstrap';

// interface CreateInstanceModalProps {
//     show: boolean;
//     onHide: () => void;
// }

interface CreateInstanceModalProps {
    show: boolean;
    onHide: () => void;
    onSubmit: (name: string) => Promise<string | undefined>;
}

// function CreateInstanceModal({ show, onHide }: CreateInstanceModalProps) {
//     const [name, setName] = useState('');

//     function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
//         event.preventDefault();
//     }

//     return (
//         <Modal show={show} onHide={onHide} centered>
//             <Form onSubmit={handleSubmit}>
//                 <Modal.Header closeButton><Modal.Title>Create instance</Modal.Title></Modal.Header>
//                 <Modal.Body>
//                     <Form.Label htmlFor="instance-name">Instance name</Form.Label>
//                     <Form.Control id="instance-name" value={name} onChange={(event) => setName(event.target.value)} maxLength={60} required />
//                     <p className="action-placeholder-note">This form is not connected yet. Nothing will be created.</p>
//                 </Modal.Body>
//                 <Modal.Footer>
//                     <Button variant="outline-secondary" onClick={onHide}>Cancel</Button>
//                     <Button type="submit" className="workflow-primary-button">Create instance</Button>
//                 </Modal.Footer>
//             </Form>
//         </Modal>
//     );
// }

function CreateInstanceModal({ show, onHide, onSubmit }: CreateInstanceModalProps) {
    const [name, setName] = useState('');
    const [busy, setBusy] = useState(false);

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        setBusy(true);
        const id = await onSubmit(name.trim());
        setBusy(false);
        if (id) {
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
                    <Form.Control
                        id="instance-name"
                        value={name}
                        onChange={(event) => setName(event.target.value)}
                        maxLength={60}
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
                        {busy ? 'Creating…' : 'Create instance'}
                    </Button>
                </Modal.Footer>
            </Form>
        </Modal>
    );
}

export default CreateInstanceModal;