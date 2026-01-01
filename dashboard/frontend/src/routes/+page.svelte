<script lang="ts">
	let status: { connected: boolean; device: string | null } | null = $state(null);
	let statusError: string | null = $state(null);
	let flashResult: { success: boolean; message?: string; error?: string } | null = $state(null);
	let flashing = $state(false);
	let fileInput: HTMLInputElement;

	async function checkStatus() {
		try {
			const res = await fetch('/api/status');
			status = await res.json();
			statusError = null;
		} catch (e) {
			statusError = 'Failed to connect to backend';
			status = null;
		}
	}

	async function flashFirmware() {
		const file = fileInput?.files?.[0];
		if (!file) return;

		flashing = true;
		flashResult = null;

		try {
			const formData = new FormData();
			formData.append('file', file);

			const res = await fetch('/api/flash', {
				method: 'POST',
				body: formData
			});

			flashResult = await res.json();
		} catch (e) {
			flashResult = { success: false, error: 'Failed to connect to backend' };
		} finally {
			flashing = false;
		}
	}

	$effect(() => {
		checkStatus();
		const interval = setInterval(checkStatus, 5000);
		return () => clearInterval(interval);
	});
</script>

<div class="min-h-screen bg-gray-900 text-white p-8">
	<h1 class="text-3xl font-bold mb-8">LED Cube Dashboard</h1>

	<!-- Status -->
	<div class="mb-8 p-4 rounded-lg bg-gray-800">
		<h2 class="text-xl font-semibold mb-2">Teensy Status</h2>
		{#if statusError}
			<p class="text-red-400">{statusError}</p>
		{:else if status}
			<p class="flex items-center gap-2">
				<span
					class="w-3 h-3 rounded-full {status.connected ? 'bg-green-500' : 'bg-red-500'}"
				></span>
				{status.connected ? `Connected (${status.device})` : 'Not connected'}
			</p>
		{:else}
			<p class="text-gray-400">Checking...</p>
		{/if}
	</div>

	<!-- Flash Firmware -->
	<div class="p-4 rounded-lg bg-gray-800">
		<h2 class="text-xl font-semibold mb-4">Flash Firmware</h2>

		<div class="flex gap-4 items-center mb-4">
			<input
				type="file"
				accept=".hex"
				bind:this={fileInput}
				class="block text-sm text-gray-400 file:mr-4 file:py-2 file:px-4 file:rounded file:border-0 file:bg-blue-600 file:text-white hover:file:bg-blue-700 file:cursor-pointer"
			/>
			<button
				onclick={flashFirmware}
				disabled={flashing}
				class="px-4 py-2 bg-blue-600 rounded hover:bg-blue-700 disabled:opacity-50 disabled:cursor-not-allowed"
			>
				{flashing ? 'Flashing...' : 'Flash'}
			</button>
		</div>

		{#if flashResult}
			<div class="p-3 rounded {flashResult.success ? 'bg-green-900' : 'bg-red-900'}">
				{#if flashResult.success}
					<p class="text-green-300">{flashResult.message}</p>
				{:else}
					<p class="text-red-300">{flashResult.error}</p>
				{/if}
			</div>
		{/if}
	</div>
</div>
