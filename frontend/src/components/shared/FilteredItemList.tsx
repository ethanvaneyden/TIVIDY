import { Badge, ListGroup } from 'react-bootstrap';
import type { Submission, Treatment } from '../../types/types';

type Item = Submission | Treatment;

interface FilteredItemListProps {
    items: Item[];
    emptyMessage: string;
    onSelect: (item: Item) => void;
}

function FilteredItemList({ items, emptyMessage, onSelect }: FilteredItemListProps) {
    if (items.length === 0) {
        return <p className="history-empty">{emptyMessage}</p>;
    }
    return (
        <ListGroup variant="flush" className="filtered-item-list">
            {items.map((item) => (
                <ListGroup.Item
                    key={item.id}
                    action
                    onClick={() => onSelect(item)}
                    className="filtered-item-row"
                >
                    <div className="d-flex justify-content-between align-items-start">
                        <div>
                            <strong>{item.name}</strong>
                            <div className="text-muted small">{item.id}</div>
                        </div>
                        <Badge bg={stateBadge(item.state)}>{stateLabel(item.state)}</Badge>
                    </div>
                </ListGroup.Item>
            ))}
        </ListGroup>
    );
}

function stateBadge(state: Item['state']): string {
    switch (state) {
        case 'completed': return 'success';
        case 'rejected': return 'danger';
        case 'inprogress': return 'primary';
        case 'assigned': return 'info';
        default: return 'secondary';
    }
}

function stateLabel(state: Item['state']): string {
    return ({
        available: 'Available',
        assigned: 'Assigned',
        inprogress: 'In progress',
        completed: 'Completed',
        rejected: 'Rejected',
    })[state];
}

export default FilteredItemList;