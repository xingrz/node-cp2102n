import binding, { Interface } from './binding';

const CP2102N_PORTS = 7;

class CP2102N {
  private intf: Interface | undefined;

  constructor(intf: Interface) {
    this.intf = intf;
  }

  async getRaw(): Promise<number> {
    return await this.intf?.get() || 0;
  }

  async get(): Promise<Record<number, boolean>> {
    const read_bits = await this.getRaw();
    const result: Record<number, boolean> = {};
    for (let i = 0; i < CP2102N_PORTS; i++) {
      result[i] = (read_bits & (1 << i)) != 0;
    }
    return result;
  }

  async setRaw(state: number, mask: number): Promise<number> {
    return await this.intf?.set(state, mask) || 0;
  }

  async set(state: Record<number, boolean>): Promise<Record<number, boolean>> {
    let write_bits = 0, write_mask = 0;
    for (let i = 0; i < CP2102N_PORTS; i++) {
      if (typeof state[i] != 'undefined') {
        write_bits |= state[i] ? (1 << i) : 0;
        write_mask |= (1 << i);
      }
    }
    const read_bits = await this.setRaw(write_bits, write_mask);
    const result: Record<number, boolean> = {};
    for (let i = 0; i < CP2102N_PORTS; i++) {
      result[i] = (read_bits & (1 << i)) != 0;
    }
    return result;
  }

  async getSerialNumber(): Promise<string | undefined> {
    return await this.intf?.getSerialNumber();
  }

  close(): void {
    this.intf?.close();
    this.intf = undefined;
  }
}

export async function openInterface(path: string): Promise<CP2102N> {
  return new CP2102N(await binding.openInterface(path));
}
