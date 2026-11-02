// Contract test against the REAL C++ server. Skipped unless LIVE_API is set:
//   (start ./build/tividy 8080)  VITE_LIVE_API=1 npm test
import { describe, expect, it } from 'vitest';
import {
    addContainer, addItem, approveContainer, completeItem, deleteInstance, getHistory, getProgress,
    getState, getStrategy, listInstances, openContainer, overrideItem, setStrategy, startInstance, startItem,
} from './workflow';
import { mapEducation, mapHospital } from './mapWorkflow';

const live = import.meta.env.VITE_LIVE_API;
describe.skipIf(!live)('frontend API layer against the live C++ backend', () => {
    it('runs the education workflow end to end', async () => {
        const id = await startInstance('education', 'Live education');
        expect(await listInstances('education')).toContain(id);

        const open = mapEducation(await getState('education', id));
        expect(open.name).toBe('Live education');
        expect(open.assignments[0].phase).toBe('Intake');

        await addContainer('education', id, 'Test 1');
        await addItem('education', id, 'A2', { studentName: 'Zed' });
        await openContainer('education', id, 'A1');
        expect(mapEducation(await getState('education', id)).assignments[0].phase).toBe('Active');
        expect(mapEducation(await getState('education', id)).assignments[0].items[0].assignee).not.toBe('');

        await expect(startItem('education', id, 'SUB-A').then(() => startItem('education', id, 'SUB-A')))
            .rejects.toThrow(/Cannot start/);

        await setStrategy('education', id, 'Random');
        expect((await getStrategy('education', id)).strategy).toBe('Random');

        const marks: Record<string, number> = { 'SUB-A': 60, 'SUB-B': 60, 'SUB-C': 60, 'SUB-D': 60, 'SUB-E': 60, 'SUB-F': 60, 'SUB-G': 30, 'SUB-H': 60 };
        for (const [item, mark] of Object.entries(marks)) {
            if (item !== 'SUB-A') await startItem('education', id, item);
            await completeItem('education', id, item, mark);
        }
        expect(mapEducation(await getState('education', id)).assignments[0].phase).toBe('Review');
        await approveContainer('education', id, 'A1');
        const rejected = mapEducation(await getState('education', id)).assignments[0].items.find((i) => i.state === 'rejected');
        expect(rejected?.id).toBe('SUB-G');
        expect(rejected?.rejectionReason).toMatch(/cannot be auto-approved/);

        await overrideItem('education', id, 'SUB-G', 45, false);
        expect(mapEducation(await getState('education', id)).assignments[0].phase).toBe('Publish');
        // 8 of the 9 submissions are done (Zed in the second assignment is still in Intake).
        const progress = await getProgress('education', id);
        expect(progress.totalItems).toBe(9);
        expect(progress.percentComplete).toBe(88);
        expect(progress.containers[0].phase).toBe('Publish');
        expect((await getHistory('education', id)).some((l) => l.includes('Marks released'))).toBe(true);

        await deleteInstance('education', id);
        expect(await listInstances('education')).not.toContain(id);
    });

    it('runs the healthcare exception path (severity 0)', async () => {
        const id = await startInstance('healthcare', 'Live hospital');
        await addItem('healthcare', id, 'W1', { name: 'New', age: 40, diagnosis: 'Flu' });
        const ward = mapHospital(await getState('healthcare', id)).wards[0];
        expect(ward.items.at(-1)).toMatchObject({ name: 'New', age: 40, diagnosis: 'Flu' });

        await openContainer('healthcare', id, 'W1');
        const severities: Record<string, number> = { 'TRT-1': 0, 'TRT-2': 5, 'TRT-3': 9, 'TRT-4': 2 };
        for (const [item, severity] of Object.entries(severities)) {
            await startItem('healthcare', id, item);
            await completeItem('healthcare', id, item, severity);
        }
        await approveContainer('healthcare', id, 'W1');
        expect(mapHospital(await getState('healthcare', id)).wards[0].items.find((i) => i.state === 'rejected')?.id).toBe('TRT-1');
        await expect(overrideItem('healthcare', id, 'TRT-1', 0, true)).rejects.toThrow(/cannot settle/);
        await overrideItem('healthcare', id, 'TRT-1', 4, false);
        expect(mapHospital(await getState('healthcare', id)).wards[0].phase).toBe('Publish');
    });
});
