<script lang="ts">
	interface ParamDef {
		key: string;
		label: string;
		type: 'float' | 'int' | 'color';
		min?: number;
		max?: number;
		default: number | string;
		step?: number;
	}

	interface Animation {
		id: number;
		name: string;
		description: string;
		params: ParamDef[];
	}

	let status: { connected: boolean; device: string | null } | null = $state(null);
	let statusError: string | null = $state(null);
	let flashResult: { success: boolean; message?: string; error?: string } | null = $state(null);
	let flashing = $state(false);
	let fileInput: HTMLInputElement;

	// Animation state
	let animations: Animation[] = $state([]);
	let currentAnimation: number | null = $state(null);
	let animationError: string | null = $state(null);
	let settingAnimation = $state(false);

	// Parameter values per animation: { animId: { paramKey: value } }
	let paramValues: Record<number, Record<string, number | string>> = $state({});

	// Debounce helper
	let debounceTimer: ReturnType<typeof setTimeout> | null = null;
	function debounce(fn: () => void, delay: number) {
		if (debounceTimer) clearTimeout(debounceTimer);
		debounceTimer = setTimeout(fn, delay);
	}

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

	async function loadAnimations() {
		try {
			const res = await fetch('/api/animations');
			const data = await res.json();
			animations = data.animations;

			// Initialize param values with defaults
			for (const anim of data.animations) {
				if (!paramValues[anim.id]) {
					paramValues[anim.id] = {};
					for (const param of anim.params) {
						paramValues[anim.id][param.key] = param.default;
					}
				}
			}
		} catch (e) {
			animationError = 'Failed to load animations';
		}
	}

	async function setAnimation(id: number, params?: Record<string, number | string>) {
		settingAnimation = true;
		animationError = null;

		const sendParams = params ?? paramValues[id] ?? {};

		try {
			const res = await fetch('/api/animation', {
				method: 'POST',
				headers: { 'Content-Type': 'application/json' },
				body: JSON.stringify({ id, params: sendParams })
			});

			const data = await res.json();

			if (data.success) {
				currentAnimation = id;
			} else {
				animationError = data.error;
			}
		} catch (e) {
			animationError = 'Failed to set animation';
		} finally {
			settingAnimation = false;
		}
	}

	function updateParam(animId: number, key: string, value: number | string) {
		if (!paramValues[animId]) paramValues[animId] = {};
		paramValues[animId][key] = value;

		// If this is the current animation, send update (debounced)
		if (currentAnimation === animId) {
			debounce(() => setAnimation(animId), 150);
		}
	}

	function getParamValue(animId: number, param: ParamDef): number | string {
		return paramValues[animId]?.[param.key] ?? param.default;
	}

	$effect(() => {
		checkStatus();
		loadAnimations();
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

	<!-- Animation Control -->
	<div class="mb-8 p-4 rounded-lg bg-gray-800">
		<h2 class="text-xl font-semibold mb-4">Animation</h2>

		{#if animations.length === 0}
			<p class="text-gray-400">Loading animations...</p>
		{:else}
			<div class="grid grid-cols-2 md:grid-cols-3 gap-3 mb-4">
				{#each animations as anim}
					<button
						onclick={() => setAnimation(anim.id)}
						disabled={settingAnimation || !status?.connected}
						class="p-3 rounded-lg text-left transition-colors
							{currentAnimation === anim.id
							? 'bg-blue-600 ring-2 ring-blue-400'
							: 'bg-gray-700 hover:bg-gray-600'}
							disabled:opacity-50 disabled:cursor-not-allowed"
					>
						<div class="font-medium">{anim.name}</div>
						<div class="text-sm text-gray-400">{anim.description}</div>
					</button>
				{/each}
			</div>

			<!-- Parameters for selected animation -->
			{#if currentAnimation !== null}
				{@const anim = animations.find((a) => a.id === currentAnimation)}
				{#if anim && anim.params.length > 0}
					<div class="mt-4 p-4 rounded-lg bg-gray-700">
						<h3 class="text-lg font-medium mb-3">{anim.name} Settings</h3>
						<div class="space-y-4">
							{#each anim.params as param}
								<div class="flex items-center gap-4">
									<label class="w-32 text-sm text-gray-300">{param.label}</label>
									{#if param.type === 'color'}
										<input
											type="color"
											value="#{getParamValue(anim.id, param)}"
											onchange={(e) =>
												updateParam(
													anim.id,
													param.key,
													e.currentTarget.value.slice(1).toUpperCase()
												)}
											disabled={settingAnimation || !status?.connected}
											class="w-12 h-8 rounded border-0 bg-gray-600 cursor-pointer disabled:opacity-50"
										/>
										<span class="text-sm text-gray-400 font-mono"
											>#{getParamValue(anim.id, param)}</span
										>
									{:else}
										<input
											type="range"
											min={param.min}
											max={param.max}
											step={param.step}
											value={getParamValue(anim.id, param)}
											onchange={(e) =>
												updateParam(anim.id, param.key, parseFloat(e.currentTarget.value))}
											disabled={settingAnimation || !status?.connected}
											class="flex-1 h-2 bg-gray-600 rounded-lg appearance-none cursor-pointer disabled:opacity-50"
										/>
										<span class="w-16 text-sm text-gray-400 text-right">
											{param.type === 'int'
												? Math.round(Number(getParamValue(anim.id, param)))
												: Number(getParamValue(anim.id, param)).toFixed(
														param.step && param.step < 1
															? Math.ceil(-Math.log10(param.step))
															: 0
													)}
										</span>
									{/if}
								</div>
							{/each}
						</div>
					</div>
				{/if}
			{/if}
		{/if}

		{#if animationError}
			<p class="mt-3 text-red-400">{animationError}</p>
		{/if}

		{#if !status?.connected}
			<p class="mt-3 text-yellow-400 text-sm">Connect Teensy to control animations</p>
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
